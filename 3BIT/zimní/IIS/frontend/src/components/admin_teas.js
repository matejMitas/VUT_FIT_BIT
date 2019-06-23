import React, {Component} from 'react';
import {Link, Redirect} from "react-router-dom";
import axios from 'axios';
//
import {API_BASE, ROUTE_PREFIX, getUser, on403} from '../const.js'
// komponenty
import withFetching from './hoc_fetching.js'

class AdminTeasPage extends Component {
    constructor(props) {
        super(props)
    }

    componentDidMount() {
        //window.confirm("Press a button!");
    }

    render() {
        //console.log(this.props.data)
        var teas = this.props.data ? this.props.data : []

        return (
            <div>
                <nav className="subHeader__nav--small">
                    <div className="layout__inner">
                        <Link to={`${ROUTE_PREFIX}admin`}>Administrace</Link>
                    </div>
                </nav>
                <div className="admin layout__inner">
                    <div className="admin__start__text">
                         <h1>Správa čajů</h1>
                        <small>Co by jste si přáli dělat?</small>
                    </div>

                    <div className="admin__buttons">
                        <Link className="button" to={`${ROUTE_PREFIX}admin/caje/pridat/`}>Přidat čaj</Link>
                    </div>


                    <table className="admin__table" cellSpacing="0" cellPadding="0">
                        <thead>
                            <tr>
                                <th>#</th>
                                <th>Název čaje</th>
                                <th>Role</th>
                                <th>Akce</th>
                            </tr>
                        </thead>
                        <tbody>
                            {teas.map((tea) => {
                                return (
                                    <tr key={tea.druh_id}>
                                        <td>{tea.druh_id}</td>
                                        <td>{tea.jmeno_caje}</td>
                                        <td>{tea.kategorie}</td>
                                        <td>
                                            <Link to={`${ROUTE_PREFIX}admin/caje/varky/${tea.druh_id}`} className="button__secondary">Várky</Link>
                                        </td>
                                    </tr>
                                ) 
                            })}
                        </tbody>
                    </table>
                </div>
            </div>
        )
    }
}

export const AdminTeas = withFetching(`${API_BASE}tea/`)(AdminTeasPage)

export class AdminTeasDetail extends Component {
    constructor(props) {
        super(props)
        
        this.state = {
            name               : '',
            teaId              : '',
            teaInfo            : '',
            ok                 : false,
            batches: undefined,

            druh_id: 0,
            category: 'green',
            quality: 3,
            prepar: '',
            louh: 1,
            character: '',
            supplier: '',

            country: '',
            continent: '',
            region: '',

            price: 0,
            remaining: 0,
            sale: 0
        }
    }

    async fetchTeaToUpdate() {
        try {
            // najdeme data na upravu
            var response = await axios({
                method: 'get',
                url: `${API_BASE}tea/${this.props.match.params.teaId}`,
                headers: {
                    token: getUser()['token']
                }
            })
            //console.log(response.data)
            this.setState({
                teaInfo: true,
                data: response.data,
                batches: response.data.vakra,
                name: response.data.jmeno_caje
            })
        } catch (error) {
            if (error.response.status === 403) {
                on403()
            }
        }
    }


    async componentDidMount() {
        //window.confirm("Press a button!");
        if (this.props.match.params.teaId) {
            this.setState({
                teaId: this.props.match.params.teaId,
            })
            this.fetchTeaToUpdate()
        }
    }

    handleChange = (type, event) => {
        let obj = {}
        obj[type] = event.target.value
        this.setState(obj)
    }

    handleSubmit = async (type, event) => {   
        event.preventDefault();

        var obj = {}

        if (type === 'add') {

            obj = {
                druh_id: 500000,
                jmeno_caje: this.state.name,
                dodavatel: this.state.supplier,
                kvalita: this.state.quality,
                pripravy: this.state.prepar,
                louhavani: this.state.louh,
                charakter: this.state.character,
                kategorie: this.state.category,
                //
                nazev_zeme: this.state.country,
                kontinent: this.state.continent,
                nazev_oblasti: this.state.region,
                //
                cena: this.state.price,
                zbyvajici_mnozstvi: this.state.remaining,
                sleva: 0
            }
            try {
                await axios({
                    method: 'post',
                    url: `${API_BASE}tea/`,
                    headers: {
                        token: getUser()['token']
                    },
                    data: obj
                })
                this.setState({ok: true})
            } catch (error) {
                if (error.response.status === 400) {
                    alert(`Nepodařilo se vytvořit uživatele - ${error.response.data}`)
                } else {
                    on403()
                }
            }
        }
    }

    render() {
        //var authorities = getAuthorities()
        // console.log(this.state.teaInfo)

        if (this.state.ok) {
            return <Redirect to={`${ROUTE_PREFIX}admin/caje`}></Redirect>
        }

        var navBar = (
            <nav className="subHeader__nav--small">
                <div className="layout__inner">
                    <Link to={`${ROUTE_PREFIX}admin`}>Administrace</Link>
                    <span className="subHeader__separator">/</span>
                    <Link to={`${ROUTE_PREFIX}admin/caje`}>Čaje</Link>
                </div>
            </nav>
        )

        var ctx
        if (this.state.data) {
            ctx = <BatchRow key={0} props={{newItem: true, data: this.state.data}}></BatchRow>
        }     

        if (this.state.teaId) {
            // varky
            var batches = this.state.batches ? this.state.batches : []
            return (
                <div>
                    {navBar}
                    <div className="admin layout__inner">
                        <div className="admin__start__text">
                            <h1>Upravit várky čaje ({this.state.name})</h1>
                        </div>

                        <h3>Přidat várku</h3>
                        <table className="admin__table" cellSpacing="0" cellPadding="0">
                            <thead>
                                <tr>
                                    <th>#</th>
                                    <th>Cena za (5g)</th>
                                    <th>Zbývající množství (g)</th>
                                    <th>Akce</th>
                                </tr>
                            </thead>
                            <tbody>
                                {ctx}
                            </tbody>
                        </table>

                        <h3>Smazat / Upravit</h3>
                        <table className="admin__table" cellSpacing="0" cellPadding="0">
                        <thead>
                            <tr>
                                <th>#</th>
                                <th>Cena za (5g)</th>
                                <th>Zbývající množství (g)</th>
                                <th>Akce</th>
                            </tr>
                        </thead>
                        <tbody>
                            {batches.map((batch) => {
                                batch.newItem = false;
                                return (
                                    <BatchRow key={batch.varka_id} props={batch}></BatchRow>
                                ) 
                            })}
                        </tbody>
                    </table>
                    </div>
                </div>
            )

        
        } else {
            return (
                <div>
                    {navBar}
                    <div className="admin admin__start layout__inner">
                        <div className="admin__start__text">
                            <h1>Přidat čaj</h1>
                        </div>

                        <form onSubmit={(event) => this.handleSubmit('add', event)}>
                            <span>
                        <label>
                            <span>Název čaje</span>
                                <input 
                                    type="text" 
                                    required 
                                    maxLength="64"
                                    value={this.state.name} 
                                    onChange={(event) => this.handleChange('name', event)} 
                                />
                            </label>
                            <label>
                                <span>Dodavatel</span>
                                <input 
                                    type="text" 
                                    required 
                                    maxLength="64"
                                    value={this.state.supplier} 
                                    onChange={(event) => this.handleChange('supplier', event)} 
                                />
                            </label>
                            <label>
                                <span>kategorie</span>
                                    <select onChange={(event) => this.handleChange('category', event)}>
                                        <option key="green" value="green">zelené</option>
                                        <option key="yellow" value="yellow">žluté</option>
                                        <option key="black" value="black">černé</option>
                                        <option key="puerh" value="puerh">puerh</option>
                                        <option key="white" value="white">white</option>
                                        <option key="oolongy" value="oolongy">oolongy</option>
                                    </select>
                            </label>
                            <label>
                                <span>Kvalita (1-3)</span>
                                <input 
                                    type="text" 
                                    required 

                                    pattern="[1-3]{1}"
                                    value={this.state.quality} 
                                    onChange={(event) => this.handleChange('quality', event)} 
                                />
                            </label>
                            <label>
                                <span>Přípravy</span>
                                <input 
                                    type="text" 
                                    required 
                                    maxLength="64"
                                    value={this.state.prepar} 
                                    onChange={(event) => this.handleChange('prepar', event)} 
                                />
                            </label>
                            <label>
                                <span>Louhování (min)</span>
                                <input 
                                    type="number" 
                                    required 
                                    maxLength="4"
                                    value={this.state.louh} 
                                    onChange={(event) => this.handleChange('louh', event)} 
                                />
                            </label>
                            <label>
                                <span>Charakter čaje</span>
                                <textarea 
                                    style={{'minHeight': '200px'}}
                                    required
                                    value={this.state.character} 
                                    onChange={(event) => this.handleChange('character', event)} >
                                    
                                </textarea>

                            </label>
                            <br/>
                            <label>
                                <span>Název Země</span>
                                <input 
                                    type="text" 
                                    required 
                                    maxLength="24"
                                    value={this.state.country} 
                                    onChange={(event) => this.handleChange('country', event)} 
                                />
                            </label>
                            <label>
                                <span>Kontinent</span>
                                <input 
                                    type="text" 
                                    maxLength="16"
                                    value={this.state.continent} 
                                    onChange={(event) => this.handleChange('continent', event)} 
                                />
                            </label>
                            <label>
                                <span>Název oblasti</span>
                                <input 
                                    type="text" 
                                    maxLength="16"
                                    value={this.state.region} 
                                    onChange={(event) => this.handleChange('region', event)} 
                                />
                            </label>
                            <br/>
                            <br/>
                            <h3>První várka</h3>
                            <label>
                                <span>Cena (za 5g)</span>
                                <input 
                                    type="number" 
                                    required 
                                    min="1"
                                    maxLength="24"
                                    value={this.state.price} 
                                    onChange={(event) => this.handleChange('price', event)} 
                                />
                            </label>
                            <label>
                                <span>Množství</span>
                                <input 
                                    type="number" 
                                    required 
                                    min="5"
                                    step="5"
                                    maxLength="24"
                                    value={this.state.remaining} 
                                    onChange={(event) => this.handleChange('remaining', event)} 
                                />
                            </label>
                            <br/>
                        </span>


                            <input type="submit" value="Přidat čaj" className="button"/>
                            <Link to={`${ROUTE_PREFIX}admin/caje`} className="button__secondary">Zrušit akci</Link>
                        </form>
                    </div>
                </div>
            )
        }
    }
}



class BatchRow extends Component {
    constructor(props) {
        super(props)
        this.state = {
            batchAmount: 1,
            batchPrice: 1,
            add: false
        }
    }

    handleChange = (type, event) => {
        let obj = {}
        obj[type] = event.target.value
        this.setState(obj)
    }

    componentDidMount() {
        // // pridano
        if (this.props.props.newItem) {
            this.setState({add: true})
        } else {
            this.setState({
                batchAmount: this.props.props.zbyvajici_mnozstvi,
                batchPrice: this.props.props.cena
            })
        }        
    }

    handleButton = async (type, event) => {
        if (this.state.batchPrice < 0 || this.state.batchAmount < 0) {
            alert('Nelze vytvořit s negativním množstvím')
            window.location.reload()
        }

        if (!this.state.batchAmount || !this.state.batchPrice) {
            alert(`Nelze přidat/upravit várku s nulovými hodnotami`)
        } else {
            if (type === 'edit') {
                try {
                   await axios({
                        method: 'put',
                        url: `${API_BASE}tea/${this.props.props.varka_id}`,
                        headers: {
                            token: getUser()['token']
                        },
                        data: {
                            cena: this.state.batchPrice,
                            zbyvajici_mnozstvi: this.state.batchAmount,
                            sleva: 0

                        }
                    })
                    alert('Várka upravena')
                } catch (error) {
                    on403()
                }
            } else if (type === 'add') {
                var data = this.props.props.data
                
                var obj = {
                    druh_id: data.druh_id,
                    jmeno_caje: data.jmeno_caje,
                    dodavatel: data.dodavatel,
                    kvalita: data.kvalita,
                    pripravy: data.pripravy,
                    louhavani: data.louhavani,
                    charakter: data.charakter,
                    kategorie: data.kategorie,
                    //
                    nazev_zeme: data.nazev_zeme,
                    kontinent: data.kontinent,
                    nazev_oblasti: data.nazev_oblasti,
                    //
                    cena: this.state.batchPrice,
                    zbyvajici_mnozstvi: this.state.batchAmount,
                    sleva: 0
                }

                try {
                    await axios({
                        method: 'post',
                        url: `${API_BASE}tea/`,
                        headers: {
                            token: getUser()['token']
                        },
                        data: obj
                    })
                    alert('Várka vytvořena')
                } catch (error) {
                    if (error.response.status === 400) {
                        alert(`Nepodařilo se přidat várku}`)
                    } else {
                        on403()
                    }
                }
            } else if (type === 'delete') {
                if (window.confirm('Opravdu chcete smazat várku?')) {
                    try {
                        await axios({
                            method: 'delete',
                            url: `${API_BASE}tea/${this.props.props.varka_id}`,
                            headers: {
                                token: getUser()['token']
                            },
                            data: {}
                        })
                        alert('Várka smazána')
                    } catch (error) {
                        if (error.response.status === 400) {
                            alert(`Nepodařilo se smazat várku}`)
                        } else {
                                on403()
                        }
                    }
                } 
            }  
        }
        window.location.reload()
    }

    render() {
        var data = this.props.props
        var control
        //
        if (this.state.add) {
            control = (
                <td>
                    <button className="button" onClick={(event) => this.handleButton('add', event)}>Přidat</button>
                </td>
            )
        } else {
            control = (
                <td>
                    <button className="button__secondary" onClick={(event) => this.handleButton('edit', event)}>Upravit</button>
                    <button className="button__secondary" onClick={(event) => this.handleButton('delete', event)}>Smazat</button>
                </td>
            )
        }
        //
        return (
            <tr>
                <td>{data.varka_id}</td>
                <td>
                    <input 
                        type="number" 
                        required 
                        min="1"
                        step="1"
                        maxLength="8"
                        value={this.state.batchPrice}
                        onChange={(event) => this.handleChange('batchPrice', event)}/>
                </td>
                <td>
                    <input 
                        type="number" 
                        required 
                        min="5"
                        step="5"
                        maxLength="8"
                        value={this.state.batchAmount}
                        onChange={(event) => this.handleChange('batchAmount', event)}/>
                </td>
                {control}
            </tr>
        )
    }
}